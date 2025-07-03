import { Component, Input } from '@angular/core';
import { BreakpointService } from '../services/breakpoint.service';

@Component({
  selector: 'app-breakpoints',
  templateUrl: './breakpoints.component.html',
  styleUrls: ['./breakpoints.component.css'],
  standalone: false
})
export class BreakpointsComponent {
  @Input() set breakpoint(value: [string, number]) {
    this.breakpointService.currentBreakpoint = value;
  }

  @Input() set frames(value: any[]) {
    this.breakpointService.currentFrames = value;
  }

  constructor(public breakpointService: BreakpointService) {}

  setBreakpoint(bp: [string, number]): void {
    this.breakpointService.setbreakpoint(bp);
  }

  breakpointClass(bp: [string, number]): string {
    return this.breakpointService.breakpointclass(bp);
  }

  breakpointDefaultClass(bp: [string, number], extra: string): string {
    return this.breakpointService.breakpointdefaultclass(bp, extra);
  }
}
