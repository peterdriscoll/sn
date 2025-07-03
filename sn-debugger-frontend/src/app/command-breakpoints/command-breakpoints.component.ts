import { Component } from '@angular/core';
import { BreakpointService } from '../services/breakpoint.service';

@Component({
  selector: 'app-command-breakpoints',
  templateUrl: './command-breakpoints.component.html',
  styleUrls: ['./command-breakpoints.component.css'],
  standalone: false
})
export class CommandBreakpointsComponent {
  constructor(public breakpointService: BreakpointService) {}

  setbreakpoint(bp: [string, number]): void {
    this.breakpointService.setbreakpoint(bp);
  }

  breakpointclass(bp: [string, number]): string {
    return this.breakpointService.breakpointclass(bp);
  }
}
