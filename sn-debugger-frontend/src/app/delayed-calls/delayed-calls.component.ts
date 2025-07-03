import { Component, Input } from '@angular/core';
import { BreakpointService } from '../services/breakpoint.service';

@Component({
  selector: 'app-delayed-calls',
  templateUrl: './delayed-calls.component.html',
  standalone: false
})
export class DelayedCallsComponent {
  @Input() delayedcalls: any[] = [];
  @Input() countdelayedcalls!: number;

  constructor(public breakpointService: BreakpointService) {}

  setbreakpoint(bp: [string, number]) {
    this.breakpointService.setbreakpoint(bp);
  }

  breakpointclass(bp: [string, number]): string {
    return this.breakpointService.breakpointclass(bp);
  }

  loaddelayedcalls(update: boolean) {}

  trustAsHtml(value: string): string {
    return value;
  }
}
